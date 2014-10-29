var _ = require('underscore');

/*!
 * \brief Convert an array of objects to an array of arrays (like a table, rows
 * first).
 *
 * \param attrs List of attributes to extract from the objects in 'data'.
 * \param data List of objects that form the data in the table.
 */
exports.selectTable = function(attrs, data) {
    return _.map(
        data,
        function(obj) {
            return _.map(attrs, function(attr) { return obj[attr]; });
        }
        );
};

/*
 * Generate a HTTP GET query string including the leading ?.  params is a map
 * of (parameter name -> value).
 */
exports.queryString = function(params) {
    var varList = [];
    for(p in params)
        varList.push(p + '=' + encodeURI(params[p]));

    return '?' + varList.join('&');
};

/*!
 * \brief Execute a function on the values of an object.
 *
 * \param f Function to execute on each value.  Accepts the values as the first
 * parameter and the key as the second parameter.
 * \returns A new object with the same keys as obj but values mutated by f.
 */
exports.mapValues = function(obj, f) {
    var result = {};
    for(key in obj) result[key] = f(obj[key], key);
    return result;
};

/*!
 * \brief Join two arrays of arrays to make a single array of arrays.
 * Analogous to 'join' in relational databases.
 *
 * arr1 and arr2 must be the same length.
 *
 * \param arr1 The array of arrays that will be the 'left' of the result.
 *
 * \param arr2 The array of arrays that will be the 'right' of the result.
 *
 * \returns An array of arrays, each subarray the concatenation of subarrays in
 * arr1 and arr2.
 */
exports.join = function(arr1, arr2) {
    var out = [];
    for(var i = 0; i < Math.max(arr1.length, arr2.length); ++i)
        out.push(arr1[i].concat(arr2[i]));
    return out;
};

/*!
 * \brief Select the first argument that is not null.  Call with any number of
 * arguments.
 *
 * \returns The first non-null argument, or null if all arguments are null.
 */
exports.coalesce = function() {
    for(a in arguments)
        if(!_.isNull(arguments[a]) && !_.isUndefined(arguments[a]))
            return arguments[a];
    return null;
};

exports.errorString = function(errors) {
    if(_.isObject(errors))
        return 'Please fix the following errors: ' +
            _.values(errors).join(', ') + '.';
    return errors;
};

