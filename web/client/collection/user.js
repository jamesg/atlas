var Backbone = require('backbone');

var api = require('../service/api');

var User = require('../model/user').User;

exports.UserCollection = Backbone.Collection.extend(
    {
        model: User,
        sync: api.backboneSyncFunction({ read: 'user_list' })
    }
    );

