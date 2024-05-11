(function ($) {
  /**
   * Spinner for download time
   */
  Backdrop.behaviors.ip_deny_countrySpinner = {
    attach: function (context) {
      var submit = $('input#edit-submit');
      var spinner = $('#spinner');
      var title = 'Please wait, downloading data...';
      $(submit, context).on('click', function (event) {
        spinner.addClass('waiting').attr('title', title);
        submit.attr('title', title).disabled = true;
      });
    }
  };
})(jQuery);
