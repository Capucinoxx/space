const puppeteer = require('puppeteer');


const config = {
  port: '8000',
  screenshot: 'screenshot.png'
};


for (let i = 2; i < process.argv.length; i++) {
  const [key, value] = process.argv[i].split('=');

  switch (key) {
    case '--port':
      config.port = value;
      break;
    case '--path':
      config.screenshot = value;
      break;
  }
}

(async () => {
  const browser = await puppeteer.launch();
  const page = await browser.newPage();
  await page.goto(`http://localhost:${config.port}`);
  await page.screenshot({ path: `${config.screenshot}` });
  await browser.close();
})();