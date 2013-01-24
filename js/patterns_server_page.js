jQuery(document).ready(function() {
    
    //when click on download link, download times grown by 1.
    var download_link = jQuery(".pattern_row .download-link");
    download_link.click(function() {
        var download_times = jQuery(this).parent().next();
        var numb = Number(download_times.text()) + 1; 
        download_times.children().text(numb);
    });

    //use moment.js to format upload time.
    jQuery(".pattern_row .upload-time").text(function(){
        return moment.unix(jQuery(this).attr("value")).fromNow();
    });

    //pattern file content.
    //jQuery("#one_pattern_file_content").hide();
    //jQuery("#one_pattern_file_descript").click(function() {
    //    jQuery("#one_pattern_file_content").fadeToggle('slow');
    //});
});
