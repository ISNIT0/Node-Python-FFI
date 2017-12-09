const lib = require('./build/Release/pythonffi');

//https://docs.python.org/3/c-api/arg.html#c.Py_BuildValue
const formatType = {
    'string': 's',
    'number': 'd',
    'object': 's'
};

function exec(pattern) {
    const func = new Function(`return ${pattern};`);
    return func();
};


const fn = function callPythonFunc(pythonFunctionName, ...args) {
    return new Promise((resolve, reject) => {
        const func = lib[pythonFunctionName];
        const format = args.map(val => formatType[typeof val]).join('');
        args = args.map(arg => {
            if (typeof arg === 'object') { //TODO: support objects (Arrays and Key Value Stores)
                return JSON.stringify(arg);
            } else {
                return arg;
            }
        });
        try {
            let res = lib.execPythonFunction(pythonFunctionName, `(${format})`, args);

            resolve(res);
        } catch (err) {
            reject(err);
        }
    });
};

fn.andParse = function callPythonFuncAndParse() {
    return fn.apply(this, arguments).then(result => {
        try {
            result = exec(result);
        } catch (err) {}
        return result;
    });
};

// fn.exec = function execPythonString(pythonString) {
//     return new Promise((resolve, reject) => {
//         try {
//             let res = lib.execString(pythonString);

//             resolve(res);
//         } catch (err) {
//             reject(err);
//         }
//     });
// };

module.exports = fn;