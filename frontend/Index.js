import { analyzeTranscript } from './transcriptHandler.js';

// Assuming you're using a command framework
bot.command('analyze', async (ctx) => {
  const transcriptText = ctx.message.text.replace('/analyze', '').trim();
  const analysis = await analyzeTranscript(transcriptText);
  ctx.reply(analysis);
});
