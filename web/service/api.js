var _ = require('underscore');
var Q = require('q');

/*!
 * \brief Make a JSON remote procedure call.
 *
 * \param options Map of JSONRPC request options.  Keys are method, params,
 * xhr.
 *
 * \returns A Q promise.
 */
exports.rpc = function(options) {
    var deferred = Q.defer();

    var req = _.has(options, 'xhr')?options.xhr:new XMLHttpRequest;
    var reqListener = function() {
        console.log('api response: ' + this.responseText);
        var jsonIn = JSON.parse(this.responseText);
        if(jsonIn['result'])
            deferred.resolve(jsonIn['result']);
        else
            deferred.reject(jsonIn['error']);
    }

    var requestContent = JSON.stringify(_.pick(options, 'method', 'params'));

    console.log('api request: ' + requestContent);

    req.open('post', '/api_call', true);

    //if(window.localStorage.getItem('token'))
        //req.setRequestHeader('Authorization', window.localStorage.getItem('token'));
    req.onload = reqListener;
    req.send(requestContent);

    return deferred.promise;
};

exports.rpcFunction = function(api_function) {
    return function() {
        return exports.rpc({
            method: _.flatten(
                _.map(api_function, function(c) {
                    return (/[A-Z]/.test(c))?('_' + c.toLowerCase()):c;
                })
                ).join(''),
            params: _.toArray(arguments)
        });
    };
};

/*!
 * \brief Generate a function that can be used as a Backbone model sync function.
 *
 * The resulting function can be used for specialisations of Backbone.Model and
 * of Backbone.Collection.
 *
 * \param rpcFunctions Map of Backbone methods to RPC function names.  Keys are
 * create, read, update and remove.
 */
exports.backboneSyncFunction = function(rpcFunctions) {
    return function sync(method, model, options) {
        if(_.has(rpcFunctions, method))
        {
            var function_ = _.isFunction(rpcFunctions[method])?
                rpcFunctions[method]:
                _.partial(
                        exports.rpc,
                        {
                            method: rpcFunctions[method],
                            params: [model.toJSON()]
                        }
                        );
            return function_().then(
                function(data) {
                    console.log('sync success', data);
                    options.success(data);
                    return data;
                },
                function(err) {
                    console.log('sync error', err);
                    options.error(err);
                    return err;
                }
                );
        }
        else
            throw 'no function for method ' + method;
    }
};

