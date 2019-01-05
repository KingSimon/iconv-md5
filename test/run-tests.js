'use strict';
const assert = require('assert');
const addon = require('../lib/addon');


const tests = [
    `<OP>0</OP>
<DATE>2019.01.05 12:16:16</DATE>
<PID>au123456</PID>
<CID>aoyou</CID>
<NUM>TS1234567890CN</NUM>`,
    `<OP>0</OP>
<DATE>2019.01.05 12:16:16</DATE>
<PID>au123456</PID>
<CID>aoyou</CID>
<NUM>TS1234567890CN你好</NUM>`
];
const results = [
    '9094c0939cc06b4d7405237ef5f47974',
    '0c07f38cb435790222a46c3ad6ec3595',
];
const pwd = 'AU89017816';

for (let i = 0; i < tests.length; i++) {
    const result = addon.convertMD5(tests[i] + pwd);
    assert.equal(result, results[i]);
}
