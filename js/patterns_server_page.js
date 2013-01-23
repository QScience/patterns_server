jQuery(document).ready(function() {
    var download_link = jQuery('a.download-link');
    var download_times = jQuery('strong.download-times');
    var info_link = jQuery('a.info-link');
    //alert(info_link[0].firstChild.nodeValue);
    //alert(table_a[1].firstChild.nodeValue);
    //alert(table_td[5].firstChild.nodeValue);
    //alert(download_times.length);
    //
    var d_l_all = document.getElementsByTagName("a");
    //alert(d_l_all.length);
    download_link.click(function() {
        //alert(this.parent("td").next().firstChild.nodeValue);
        var numb = Number(download_times[0].firstChild.nodeValue) + 1; 
        download_times[0].html(numb);
        //return false;
    });
});
