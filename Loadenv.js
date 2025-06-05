import dotenv from 'dotenv';

// Load env variables from .env file
dotenv.config();

const requiredVars = [
  'CLOUDFLARE_ACCOUNT_ID',
  'CLOUDFLARE_API_TOKEN',
];

function validateEnv() {
  const missingVars = requiredVars.filter((key) => !process.env[key]);

  if (missingVars.length > 0) {
    console.error(`❌ Missing environment variables: ${missingVars.join(', ')}`);
    process.exit(1); // Fail fast
  } else {
    console.log('✅ All required environment variables loaded.');
  }
}

// Call validation right away
validateEnv();

// Export env vars for use elsewhere if you want
export const config = {
  CLOUDFLARE_ACCOUNT_ID: process.env.CLOUDFLARE_ACCOUNT_ID,
  CLOUDFLARE_API_TOKEN: process.env.CLOUDFLARE_API_TOKEN,
};
