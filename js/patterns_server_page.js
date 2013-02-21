jQuery(document).ready(function() {
    
    //when click on download link, download times grown by 1.
    var download_link = jQuery(".pattern_row .download-link");
    download_link.click(function() {
        var download_times = jQuery(this).parent().prev().prev();
        var numb = Number(download_times.text()) + 1; 
        download_times.children().text(numb);
    });

    //use moment.js to format upload time.
    jQuery(".pattern_row .upload-time").text(function(){
        return moment.unix(jQuery(this).attr("value")).fromNow();
    });

    //ajax
    jQuery(".info-link").click(function(){
        var url_div = jQuery(this).attr("href");
        url_div += " #one_pattern_div";
        jQuery("#one_pattern_div").load(url_div); 
        return false;
    });

    //Rate
    var rate_link = jQuery(".pattern_row .rate-link");
    rate_link.click(function() {
        var rate_times = jQuery(this).parent().prev().prev();
        var numb = Number(rate_times.text()) + 1; 
        rate_times.children().text(numb);

        var url_div = jQuery(this).attr("href");
        url_div += " #one_pattern_div";
        jQuery("#one_pattern_div").load(url_div); 
        return false;
    });
});
