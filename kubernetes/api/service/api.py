import flask, os
from flask import request, jsonify

app = flask.Flask(__name__)
app.config["DEBUG"] = True

colour = os.environ['COLOUR']
pod = os.environ['POD_NAME']
print("deployment colour: %s" % colour)

response = {
    'deployment': colour,
    'pod': pod
}

@app.route('/healthz', methods=['GET'])
def health():
    return 'OK'

# route to return deployment info.
@app.route('/', methods=['GET'])
def api_all():
    return jsonify(response)

if __name__ == "__main__":
    app.run(host=os.getenv('IP', '0.0.0.0'), 
            port=int(os.getenv('PORT', 5001)))
