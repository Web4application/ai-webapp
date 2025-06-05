const fetch = require('node-fetch');

async function testProxy() {
  const res = await fetch('http://localhost:3000/api/proxy/v1/models', {
    headers: {
      Authorization: 'Bearer your-super-secret-client-token',
    },
  });

  if (!res.ok) {
    console.error('Proxy test failed:', res.status, await res.text());
    process.exit(1);
  }

  const data = await res.json();
  console.log('Proxy test success:', data);
}

testProxy();
