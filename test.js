const python = require('./index.js');

python('myabs', 3)
    .then(result => console.info(`Got Response: [${result}] [${typeof result}]`))
    .catch(err => console.error(`Failed to execute method: `, err));

// python.exec('return 4')
//     .then(result => console.info(`Got Exec Response: [${result}] [${typeof result}]`))
//     .catch(err => console.error(`Failed to exec: `, err));