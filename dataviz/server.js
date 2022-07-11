// Simple nodejs server which: 
// 1. connects to an Azure events hub
// 2. retrieves data from the Azure events hub
// 3. forward such data to a websocket to which simple HTML/javascript application can connect

const { EventHubConsumerClient, latestEventPosition, earliestEventPosition  } = require("@azure/event-hubs");

// :::::: CONFIGURATION ::::::

// event hub configuration
const eventHubName = "<insert event hub name>";
const eventHubEventPosition = latestEventPosition; // latest: get new data, earliest: get all data from beginning

// websocket configuration
const websocketPort = 8889;

// :::::: END CONFIGURATION ::::::

const connectionString = "<insert connection string>";

const consumerGroup = "$Default"; // name of the default consumer group

async function main() {

    // Create web socket server to send event to web client
    const WebSocketServer = require('ws').Server;
    var webSocketServer = new WebSocketServer({ port: websocketPort });

    // Create a consumer client for the event hub by specifying the checkpoint store.
    const consumerClient = new EventHubConsumerClient(consumerGroup, connectionString, eventHubName);

    // Subscribe to the events, and specify handlers for processing the events and errors.
    const subscription = consumerClient.subscribe({
            processEvents: async (events, context) => {
                if (events.length === 0) {
                    console.log(`No events received within wait time. Waiting for next interval`);
                    return;
                }

                for (const event of events) {
                    console.log(`Received event: '${event.body}' from partition: '${context.partitionId}' and consumer group: '${context.consumerGroup}'`);
                    console.log(event.body);

                    webSocketServer.clients.forEach(function each(client) {
                        client.send(JSON.stringify(event.body));
                    });
                }
                // Update the checkpoint.
                await context.updateCheckpoint(events[events.length - 1]);
            },

            processError: async (err, context) => {
                console.log(`Error : ${err}`);
            },
        },
        { startPosition: eventHubEventPosition }
    );
}

main().catch((err) => {
    console.log("Error occurred: ", err);
});