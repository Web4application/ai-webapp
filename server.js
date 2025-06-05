const express = require('express');
const bodyParser = require('body-parser');
const analyzeRoute = require('./api/analyze');
const { port } = require('./config');
const botClient = require('./bot');

const app = express();
app.use(bodyParser.json());
app.use('/api', analyzeRoute);

app.listen(port, () => {
  console.log(`API Server listening at http://localhost:${port}`);
});

botClient.login(process.env.DISCORD_TOKEN);
