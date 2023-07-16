


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

  return result;
};

const scoreboard = document.getElementById('scoreboard-data');
const ctx = document.getElementById('score-chart').getContext('2d');

const timeline_chart = new Chart(ctx, {
  type: 'line',
  options: {
    maintainAspectRatio: false,
    scales: {
      x: {
        type: 'time',
        grid: { display: false },
        ticks: {
          autoSkip: true,
          maxTicksLimit: 5,
          display: false,
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
          color: 'white',
        },
        grid: { display: false },
      }
    }
  }
});

const retrieve_data = async () => {
  return deserialize(await fetch('http://localhost:8080/scoreboard')
            .then(response => response.arrayBuffer()));
}

const update_chart_with_data = (data) => {
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

  timeline_chart.data.datasets = dataset;
  timeline_chart.update();
};

const update_scoreboard = (data) => {
  let ranking = [];
  for (const [name, {team_data}] of Object.entries(data)) {
    ranking.push({ name, score: team_data[team_data.length - 1].y });
  }
  ranking.sort((a, b) => b.score - a.score);



  scoreboard.replaceChildren(...ranking.slice(0, 10).reduce((acc, {name, score}, idx) => {
    const row = document.createElement('tr');

    const rank = document.createElement('td');
    const rank_span = document.createElement('span');
    rank_span.innerText = idx + 1 + (idx == 0 ? 'st' : idx == 1 ? 'nd' : idx == 2 ? 'rd' : 'th');
    rank.appendChild(rank_span);
    row.appendChild(rank);

    const name_td = document.createElement('td');
    name_td.innerText = name;
    row.appendChild(name_td);

    const score_td = document.createElement('td');
    score = 123456789;
    score_td.innerText = score.toString().replace(/\B(?=(\d{3})+(?!\d))/g, ' ');
    row.appendChild(score_td);

    return [...acc, row];
  }, []));
};

const update = async () => {
  const data = await retrieve_data();

  update_chart_with_data(data);
  update_scoreboard(data);
};





await update();

setInterval(async () => {
  await update();
}, 1000 * 60);


