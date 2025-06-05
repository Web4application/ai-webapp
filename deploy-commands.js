const { REST, Routes, SlashCommandBuilder } = require('discord.js');
require('dotenv').config();

const commands = [
  new SlashCommandBuilder()
    .setName('analyze')
    .setDescription('Analyze a piece of text using AI')
    .addStringOption(option =>
      option.setName('text')
        .setDescription('The text you want to analyze')
        .setRequired(true)
    )
].map(cmd => cmd.toJSON());

const rest = new REST({ version: '10' }).setToken(process.env.DISCORD_TOKEN);

(async () => {
  try {
    console.log('Registering slash command...');
    await rest.put(
      Routes.applicationCommands(process.env.CLIENT_ID),
      { body: commands }
    );
    console.log('Slash command registered!');
  } catch (error) {
    console.error('Failed to register command:', error);
  }
})();
