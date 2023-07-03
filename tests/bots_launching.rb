if ARGV.empty? || ARGV[0].to_i > 100 || ARGV[0].to_i < 1
    raise ArgumentError, "Invalid argument: number of bots must be a valid number (1-100)"
end

n_bots = ARGV[0].to_i
p "Launching #{n_bots} bots"

ids = (1..n_bots)
secrets = ids.map { |id| "bot_secret#{id}" }

def run(secret)
    Thread.new { `cd ../starter-packs/javascript && npm start -- -s #{secret}` }
end

threads = secrets.map { |secret| run(secret) }

threads.each { |t| t.join }