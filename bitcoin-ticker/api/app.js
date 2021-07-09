var express = require("express");
var bodyParser = require("body-parser");
var routes = require("./routes/routes.js");
var request = require('request');
var app = express();

app.use(bodyParser.json());
app.use(bodyParser.urlencoded({ extended: true }));

(function getLatestPrices() {
    var price = request('https://api.coindesk.com/v1/bpi/currentprice.json', { json: true }, (err, res, body) => {
    if (!err && res.statusCode == 200 && body.bpi.USD.rate_float) 
    {
        BTCUSD = body.bpi.USD.rate_float
    }
    if (!err && res.statusCode == 200 && body.bpi.GBP.rate_float) 
    {
        BTCGBP = body.bpi.GBP.rate_float
    }
    if (!err && res.statusCode == 200 && body.bpi.EUR.rate_float) 
    {
        BTCEUR = body.bpi.EUR.rate_float
    }
    console.log(BTCUSD)
    console.log(BTCGBP)
    console.log(BTCEUR)
    })
    setTimeout(getLatestPrices, 30000);
})();

routes(app);

app.set('port', (process.env.PORT || 5000));

//For avoidong Heroku $PORT error
app.get('/', function(request, response) {
    var result = 'App is running'
    response.send(result);
}).listen(app.get('port'), function() {
    console.log('App is running, server is listening on port ', app.get('port'));
});