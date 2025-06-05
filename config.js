require('dotenv').config();

module.exports = {
  discordToken: process.env.DISCORD_TOKEN,
  openaiApiKey: process.env.OPENAI_API_KEY,
  port: process.env.PORT || 3000
};
