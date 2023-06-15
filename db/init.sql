CREATE TABLE player (
    id      SERIAL      PRIMARY KEY,
    uuid    VARCHAR(15) NOT NULL,
    name    VARCHAR(15) NOT NULL,
    h       INTEGER     NOT NULL,
    s       INTEGER     NOT NULL,
    l       INTEGER     NOT NULL
);

CREATE TABLE player_scores (
    id          SERIAL              PRIMARY KEY,
    player_id   INTEGER             NOT NULL,
    timestamp   TIMESTAMPTZ         NOT NULL,
    score       DOUBLE PRECISION    NOT NULL,

    FOREIGN KEY (player_id) REFERENCES player(id)
);