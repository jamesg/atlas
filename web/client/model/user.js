var _ = require('underscore');
var Backbone = require('backbone');

var api = require('../service/api');

exports.User = Backbone.Model.extend(
    {
        defaults: {
            username: ''
        },
        idAttribute: 'user_id',
        sync: api.backboneSyncFunction(
            {
                create: 'user_save',
                delete: 'user_destroy',
                update: 'user_save'
            }
            ),
        validate: function(attrs) {
            var errors = {};
            if(attrs['username'] == '')
                errors['username'] = 'Name is required';
            if(attrs['description'] == '')
                errors['description'] = 'Description is required';
            if(!_.isEmpty(errors))
                return errors;
        }
    }
    );

