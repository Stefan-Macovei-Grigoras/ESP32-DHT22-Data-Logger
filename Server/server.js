const express = require("express");
const bodyParser = require("body-parser");
const cors = require("cors");
const http = require("http");
const WebSocket = require("ws");
require("dotenv").config();

const app = express();
const server = http.createServer(app); // Create HTTP Server
const wss = new WebSocket.Server({ server }); // Attach WebSocket to HTTP Server

const PORT = 3000;

app.use(bodyParser.json());
app.use(cors());
// WebSocket Server Logic
wss.on("connection", (ws) => {
    console.log("Client connected to WebSocket");

    ws.on("message", (message) => {
        try {
            const data = JSON.parse(message);
            console.log("Received from ESP32:", data);

            // Broadcast the data to all connected clients
            wss.clients.forEach((client) => {
                if (client.readyState === WebSocket.OPEN) {
                    client.send(JSON.stringify(data));
                }
            });
        } catch (error) {
            console.error("Error parsing message:", error);
        }
    });

    ws.on("close", () => {
        console.log("Client disconnected from WebSocket");
    });
});

server.listen(PORT, () => {
    console.log(`Server is running on http://localhost:${PORT}`);
    console.log(`WebSocket running on ws://localhost:${PORT}`);
});
