(function($){
$(document).ready(function() {

  //upload functionality ajax implementation
  var upload_form_div = $('#patterns-server-upload-form-js');
  $(upload_form_div).css('border', '2px solid #d8d8d8');
  $(upload_form_div).css('padding-left', '13px');
  $(upload_form_div).css('padding-top', '9px');

  $(upload_form_div).hide();
  $('body').delegate('a.upload-button-link', 'click', function (){
    var upload_form_div = $('#patterns-server-upload-form-js');
    var url = $('.upload-button-link').attr('href');
    $.ajax({
      url: url,
      success: function(data) {
        if ( $(upload_form_div).is(':hidden')) {
        var upload_form = $('#patterns-server-form', data).wrap("<div></div>").parent().html();
        $(upload_form_div).html(upload_form).slideDown('slow');
        Drupal.attachBehaviors($("#patterns-server-form"));
        }
        else {
        $(upload_form_div).slideUp('slow');
        }
        //console.log(data.error);
      },
      error: function(jqXHR, exception) {
        //console.log(jqXHR.status);
        if (jqXHR.status == 403) {
        $(upload_form_div).slideUp('slow');
        $(upload_form_div).html('you don\'t have the permission to upload pattern files').slideDown('slow');
        }
      }
    });
    return false;
  });


});
})(jQuery)
