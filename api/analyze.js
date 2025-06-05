const express = require('express');
const axios = require('axios');
const { openaiApiKey } = require('../config');

const router = express.Router();

router.post('/analyze', async (req, res) => {
  const { text } = req.body;

  try {
    const response = await axios.post('https://api.openai.com/v1/chat/completions', {
      model: 'gpt-3.5-turbo',
      messages: [{ role: 'user', content: `Analyze the following text: ${text}` }],
      max_tokens: 100
    }, {
      headers: {
        'Authorization': `Bearer ${openaiApiKey}`,
        'Content-Type': 'application/json'
      }
    });

    const analysisLogs = []; // You can move this to a separate service later

router.post('/analyze', async (req, res) => {
  const { text } = req.body;
  try {
    // OpenAI call...
    const analysis = response.data.choices[0].message.content.trim();

    // Store the result
    analysisLogs.push({ text, analysis, timestamp: Date.now() });

    res.json({ analysis });
  } catch (error) {
    res.status(500).json({ error: 'Failed to analyze text' });
  }
});

// Expose a new route for UI to pull analysis logs
router.get('/history', (req, res) => {
  res.json(analysisLogs);
});
