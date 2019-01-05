const addon = require('bindings')('addon');
module.exports = {
    convertMD5: function (input_str, from_code, to_code, md5_bit) {
        //回车键兼容
        input_str = (input_str || '').replace(/\r\n/g, '\n').replace(/\n/g, '\r\n');
        from_code = from_code || 'utf-8';
        to_code = to_code || 'gbk';
        if (md5_bit !== 16 || md5_bit !== 32) {
            md5_bit = 32;
        }
        return addon.convertMD5(input_str, from_code, to_code, md5_bit);
    }
};
