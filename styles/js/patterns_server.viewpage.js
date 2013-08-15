/**
 * @file 
 * patternentity view page js.
 *
 */
(function($){
$(document).ready(function() {

  $('#patterns-server-page-pattern-field').hide();

  //popup in view page.
  $('#pattern_file_popup').click(function() {
	  if ($(this).text() == 'show') {
		  $(this).text('hide');
	  }
	  else {
		  $(this).text('show');
	  }
    $('#patterns-server-page-pattern-field').slideToggle('slow');
    return false;
  });


	//upload functionality ajax implementation
	$('#patterns-server-page-pattern-field').before('<div id="patterns-server-upload-form-js" class="hero-unit"></div>');
	var upload_form_div = $('#patterns-server-upload-form-js');
	$(upload_form_div).hide();
	$('.upload-button-link').bind('click', function (){
		var url = $('.upload-button-link').attr('href');
		$.ajax({
			url: url,
			success: function(data) {
				if ( $(upload_form_div).is(':hidden')) {
					var upload_form = $('#patternentity-form', data).wrap("<div></div>").parent().html();
					$(upload_form_div).html(upload_form).slideDown('slow');
				}
				else {
					$(upload_form_div).slideUp('slow');
				}
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

	//tags before content
	var content = $('#patterns-server-page-pattern-field');
	var tags = $('.field-type-taxonomy-term-reference');
	$(content).before($(tags));

});
})(jQuery)
