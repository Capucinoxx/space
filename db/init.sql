CREATE TABLE player (
  id      SERIAL            PRIMARY KEY,
  name    VARCHAR(15)       NOT NULL UNIQUE,
  secret  VARCHAR(15)       NOT NULL UNIQUE,
  h       DOUBLE PRECISION  NOT NULL,
  s       DOUBLE PRECISION  NOT NULL,
  l       DOUBLE PRECISION  NOT NULL
);

CREATE TABLE player_scores (
  id        SERIAL            PRIMARY KEY,
  player_id INTEGER           NOT NULL,
  timestamp TIMESTAMPTZ       NOT NULL DEFAULT NOW(),
  score     BIGINT            NOT NULL,

  FOREIGN KEY (player_id) REFERENCES player(id)
);