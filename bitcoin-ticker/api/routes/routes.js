var appRouter = function (app) {
    app.get("/", function(req, res) {
      res.status(200).send("TickerBoxx Bitcoin Price API");
    });
    app.get("/api/v1/btc", function(req, res) {
        var data =({
            BTCUSD: BTCUSD,
            BTCGBP: BTCGBP,
            BTCEUR: BTCEUR
        })
        res.status(200).send(data);
      });
  }
  
  module.exports = appRouter;