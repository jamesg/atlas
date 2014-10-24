var _ = require('underscore');
var domjs = require('domjs/lib/html5')(document);

/*!
 * \brief Build a confirmation button.
 *
 * \pre Called from within a domjs template.
 * \returns A domjs element.
 */
exports.confirmButton = function(text, callback) {
    var element = span({ class: 'confirmbutton' });

    var firstTemplate = function() {
        element(
            button(
                { class: 'pure-button', onclick: buildSecond },
                _.isFunction(text)?text():text
                )
            );
    };
    var secondTemplate = function() {
        element(
            _.isFunction(text)?text():text,
            ': ',
            button(
                {
                    class: 'pure-button pure-button-primary',
                    onclick: (function() {
                        buildFirst();
                        callback();
                    }).bind(this)
                },
                'Yes'
                ),
            button({ class: 'pure-button', onclick: buildFirst }, 'No')
            );
    };
    var buildFirst = function() {
        element().innerHTML = '';
        domjs.build(firstTemplate);
    };
    var buildSecond = function() {
        element().innerHTML = '';
        domjs.build(secondTemplate);
    };

    buildFirst();

    return element;
};

exports.icon = function(icon) {
    return span(
            {
                class: 'oi',
                'data-glyph': icon,
                title: icon,
                'aria-hidden': true
            },
            ' '
        );
};

exports.inlineInput = function(label_, input_) {
    return div(
            { class: 'pure-control-group' },
            label({ for: input_().name }, label_),
            input_
            );
};

exports.saveButton = function(text) {
    if(_.isUndefined(text))
        text = 'Save';
    return button(
        { type: 'submit', class: 'pure-button pure-button-primary' },
        exports.icon('data-transfer-download'),
        text
        );
};

