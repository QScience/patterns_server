jQuery(document).ready(function() {
    var pattern_row = jQuery('#pattern_row_1');
    //alert(pattern_row.length);
    //alert(pattern_row.length);
    var s = "#pattern_row_1";
    var download_link = jQuery("s .download-link");
    var download_times = jQuery('#pattern_row_1 .download-times');
    //alert(download_link.parents("#pattern_row_1").find(".download-times").text());
    download_link.click(function() {
    //    alert(this.parents("#pattern_row_1").find(".download-times").text());
        //var numb = Number(download_times.text()) + 1; 
        //download_times.html(numb);
       // return false;
    });
});
