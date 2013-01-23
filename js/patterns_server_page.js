jQuery(document).ready(function() {
    
    //when click on download link, download times grown by 1.
    var download_link = jQuery(".pattern_row .download-link");
    download_link.click(function() {
        var download_times = jQuery(this).parent().next();
        var numb = Number(download_times.text()) + 1; 
        download_times.children().text(numb);
    });

    //
});
