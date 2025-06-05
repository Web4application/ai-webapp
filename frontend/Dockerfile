# Use official Node.js LTS image
FROM node:18-alpine

# Create app directory
WORKDIR /usr/src/app

# Install dependencies first (leveraging caching)
COPY package*.json ./
RUN npm ci --only=production

# Copy app source code
COPY . .

# Expose port your bot listens on (adjust as needed)
EXPOSE 3000

# Run the bot
CMD ["node", "index.js"]
