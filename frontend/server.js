const express = require('express');
const bodyParser = require('body-parser');
const analyzeRoute = require('./api/analyze');
const { port } = require('./config');

const app = express();

app.use(bodyParser.json());
app.use('/api', analyzeRoute);

app.listen(port, () => {
  console.log(`API running at http://localhost:${port}`);
});

fetch('http://localhost:3000/api/analyze', {
  method: 'POST',
  headers: { 'Content-Type': 'application/json' },
  body: JSON.stringify({ text: 'Your input here' })
})
.then(res => res.json())
.then(data => console.log(data.analysis));

