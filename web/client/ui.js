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

/*!
 * \brief Construct a large menu button which, when clicked, pushes a page onto
 * the application stack.
 */
exports.menuButton = function(application, constructor, content) {
    return div(
        { class: 'pure-u-12-24 pure-u-md-8-24 pure-u-lg-6-24 pure-u-xl-4-24' },
        button(
            {
                class: 'pure-button',
                onclick: application.pushPage
                        .bind(application, constructor)
            },
            content
            )
        );
};

/*!
 * \brief Construct a 'main menu' containing large buttons.  Accepts any number
 * of buttons, typically constructed with ui.menuButton, as parameters.
 */
exports.mainMenu = function() {
    var d = div({ class: 'pure-g mainmenu' });
    for(b in arguments)
        d(arguments[b]);
    return d;
};

exports.inlineInput = function(label_, input_) {
    return div(
            { class: 'pure-control-group' },
            label({ for: input_().name }, label_),
            input_
            );
};

exports.inlineCheckInput = function(input_, label_) {
    return div(
            { class: 'pure-control-group' },
            input_,
            label({ for: input_().name }, label_)
            );
};

exports.saveButton = function(text) {
    return button(
            { type: 'submit' },
            span(exports.icon('data-transfer-download'), text)
            );
};

