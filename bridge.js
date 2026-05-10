const WebSocket = require('ws');
const net = require('net');

const wss = new WebSocket.Server({ port: 8765 });
console.log('Bridge WebSocket en écoute sur port 8765...');

wss.on('connection', (ws) => {
    console.log('Nouveau client web connecté');

    // Crée une vraie connexion TCP vers server.c
    const tcpClient = new net.Socket();
    tcpClient.connect(5000, '127.0.0.1', () => {
        console.log('Connecté au serveur C sur port 5000');
    });

    // Message du navigateur → envoie au serveur C
    ws.on('message', (cmd) => {
        console.log('Reçu du browser:', cmd.toString());
        tcpClient.write(cmd.toString());
    });

    // Réponse du serveur C → renvoie au navigateur
    tcpClient.on('data', (data) => {
        console.log('Réponse serveur C:', data.toString());
        ws.send(data.toString());
    });

    // Fermeture propre
    ws.on('close', () => {
        tcpClient.destroy();
        console.log('Client web déconnecté');
    });

    tcpClient.on('error', (err) => {
        ws.send('ERREUR: Impossible de joindre le serveur C');
    });
});
