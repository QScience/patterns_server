/**
 * @file
 */
(function($){
$(document).ready(function() {

  //autofill search box.
  function switch_autofill(text_select) {
    var value_autofill;
    switch (text_select) {
      case 'UUID': value_autofill = 'for example: 1f672fdf-24d2-64a4-e981-3d3f4cbe3d74'; break;
      case 'Title': value_autofill = 'for example: disable modules'; break;
      case 'Category': value_autofill = 'for example: system, block'; break;
      case 'Pid': value_autofill = 'e.g. 3 or any number'; break;
      case 'Author': value_autofill = 'for example: qscience, edward'; break;
      case 'Tags': value_autofill = 'for example: create, bigfile'; break;
      default: value_autofill = 'search...';
    }
    return value_autofill;
  }
  var value_input = $('#patterns-server-search input#edit-search').val();
  var value_autofill = '';
  var value_default= '';
  var autofill_sign = false;
  if (value_input.length == 0 ) {
    var text_select = $("select#edit-selected option:selected").text();
    value_default = switch_autofill(text_select);
    $('#patterns-server-search input#edit-search').autofill({
      value: value_default,
      defaultTextColor: '#666',
      activeTextColor: '#333',
    });
    autofill_sign = true;
  }
  $('#patterns-server-search input#edit-search').blur(function() {
    var text_select = $("select#edit-selected option:selected").text();
    value_autofill = switch_autofill(text_select);
    value_input = $(this).val();
    if (value_input == value_default) {
      $(this).autofill({
        value: value_autofill,
        defaultTextColor: '#666',
        activeTextColor: '#333',
      });
      autofill_sign = true;
    }
    else {
      autofill_sign = false;
    }
  });
  $("select#edit-selected").change(function () {
    $("select#edit-selected option:selected").each(function () {
      if ( autofill_sign ) {
        var text_select = $(this).text();
        value_autofill = switch_autofill(text_select);
        $('#patterns-server-search input#edit-search').autofill({
          value: value_autofill,
          defaultTextColor: '#666',
          activeTextColor: '#333',
        });
      }
    });
  })

  //search box width.
  $('input#edit-search').css('width', '65%');

  //search functionality ajax implementation
  $('#patterns-server-search #edit-submit').bind('click', function() {
    var table_wrap = $('#patterns-server-all-table-wrap');
    var page_title = $('#page-title');

    var text_select = $("select#edit-selected option:selected").text();
    var value_default = switch_autofill(text_select);
    var search_text = $('#patterns-server-search #edit-search').val().trim();
    if ( search_text == '' || search_text == value_default) {
      $(table_wrap).hide();
      $(table_wrap).html('To begin the search, type something, for example "block"').show('slow');
    }
    else {
      var search_type = $('#patterns-server-search #edit-selected option:selected').val();
      var path = $(this).attr('name');
      path += search_type + '/' + encodeURIComponent(search_text);
      //console.log(path); 
      //console.log(encodeURIComponent(search_text)); 
      $.ajax({
        url: path,
        success: function(data) {
          //console.log(data);
          var table = $('#patterns-server-all-table-wrap', data).html();
          var page_title_new = $('#page-title', data).html();

          $(page_title).html(page_title_new);
          $(table_wrap).hide();
          $(table_wrap).html(table).slideDown('slow');

          $(".patterns-server-list-table .upload-time").text(function(){
            return moment.unix($(this).attr("value")).fromNow();
          });
          $('.patterns-server-list-table-wrap').first().prepend('<div id="patterns-server-upload-form-js" class="hero-unit"></div>');
          var upload_form_div = $('#patterns-server-upload-form-js');
          $(upload_form_div).hide();
          $('.patterns-server-list-table-wrap .upload-button-link').not(':first').hide();
        }
      });
    }
    return false;
  });


});
})(jQuery)
