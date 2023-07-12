
const scoreboard_body = document.getElementById('scoreboard');

const deserialize = (data) => {
  let offset = 0;
  const decoder = new TextDecoder('unicode-1-1-utf-8');

  const result = {};  
  const length = data.byteLength;

  while (offset < length) {
    const name_length = new Uint32Array(data.slice(offset, offset + 4))[0];
    offset += 4;

    const name = new TextDecoder('unicode-1-1-utf-8').decode(data.slice(offset, offset + name_length));
    offset += name_length;

    const h = new Float64Array(data.slice(offset, offset + 8))[0];
    offset += 8;

    const s = new Float64Array(data.slice(offset, offset + 8))[0];
    offset += 8;

    const l = new Float64Array(data.slice(offset, offset + 8))[0];
    offset += 8;

    const data_length = new Uint32Array(data.slice(offset, offset + 4))[0];
    offset += 4;

    const team_data = [];

    for (let i = 0; i != data_length; i++) {
      
      const epoch = new BigUint64Array(data.slice(offset, offset + 8))[0];
      offset += 8;

      const score = new Float64Array(data.slice(offset, offset + 8))[0];
      offset += 8;

      team_data.push({ y: score, x: Number(epoch) / 1_000_000 });
    }
    
    result[name] = {
      color: [h, s, l],
      team_data: team_data,
    }
  }

  console.log(result);
  return result;
};


const ctx = document.getElementById('chart').getContext('2d');
const scoreboard = new Chart(ctx, {
  type: 'line',
  // data: { datasets: dataset },
  options: {
    scales: {
      x: {
        type: 'time',
        ticks: {
          autoSkip: true,
          maxTicksLimit: 5,
          display: false
        },
        time: { tooltipFormat: 'll HH:mm' },
        adapters: {
          date: {
            parser: function (value) { return new Date(value); }
          }
        }
      },
      y: {
        ticks: {
          autoSkip: true,
          maxTicksLimit: 5,
        },
      }
    }
  }
});


const append_data_to_chart = async (ctx) => {
  const data = deserialize(await fetch('http://localhost:8080/scoreboard')
  .then(response => response.arrayBuffer()));

  const dataset = [];

  for (const [name, {color, team_data}] of Object.entries(data)) {
    const [h, s, l] = color;
    const color_string = `hsl(${h}, ${s * 100}%, ${l * 100}%)`;

    

    dataset.push({
      label: name,
      data: team_data,
      backgroundColor: color_string,
      borderColor: color_string,
      fill: false,
      pointRadius: 0,
    });
  }
  
  scoreboard.data.datasets = dataset;
  scoreboard.update();
};


await append_data_to_chart(ctx);

setInterval(async () => {
  await append_data_to_chart(ctx);
}, 1000 * 60);


