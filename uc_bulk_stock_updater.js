// $Id: uc_bulk_stock_updater.js,v 1.3 2010/09/03 10:45:33 hiddentao Exp $

var g_uc_bulk_stock_updater_all_rows = undefined

Drupal.behaviors.uc_bulk_stock_updater = function (context) {

    g_uc_bulk_stock_updater_all_rows = $("table.uc-stock-table tbody tr");

    $("#uc_bulk_stock_updater_filter").after("<div class=\"uc_bulk_stock_updater_progress\" style=\"display:none\"></div>");

	// handle item editing
	$("input.uc_bulk_stock_updater_value").each(function(){
		$(this).data("original", $(this).val());
		$(this).change(function(){
			uc_bulk_stock_updater_submitValue(this);
		})
	});

	// filter
	$("#uc_bulk_stock_updater_filter").keyup(function(){
		uc_bulk_stock_updater_apply_filter($(this).val().toLowerCase());
	});
};


// --------------------------------------------
// Filtering
// --------------------------------------------


// track the number of filter threads that have been kicked off
// (also used to know if and when to terminate a thread early)
var g_uc_bulk_stock_updater_filter_thread_num = 1

/**
 * Apply given filter to all table rows.
 *
 * This will apply the filter via a background op (setTimeout).
 *
 * @param filter_text
 */
function uc_bulk_stock_updater_apply_filter(filter_text)
{
    g_uc_bulk_stock_updater_filter_thread_num++;
    _uc_bulk_stock_updater_apply_filter_helper(g_uc_bulk_stock_updater_filter_thread_num, 0, filter_text);
}
function _uc_bulk_stock_updater_apply_filter_helper(thread_num, row_num, filter_text)
{
    // if a newer thread has been kicked off then quit
    if (thread_num < g_uc_bulk_stock_updater_filter_thread_num)
        return;

    $("#uc_bulk_stock_updater_filter").next("div.uc_bulk_stock_updater_progress").show();

    var row = g_uc_bulk_stock_updater_all_rows.get(row_num);
    if (0 < $(row).find("span[id*=" + filter_text + "]").size()) {
        $(row).show();
    } else {
        $(row).hide();
    }

    row_num++;
    if (g_uc_bulk_stock_updater_all_rows.size() > row_num)
        setTimeout("_uc_bulk_stock_updater_apply_filter_helper(" + thread_num + "," + row_num + ",'" + filter_text + "')", 0);
    else
        $("#uc_bulk_stock_updater_filter").next("div.uc_bulk_stock_updater_progress").hide();
}


// --------------------------------------------
// AJAX call
// --------------------------------------------


function uc_bulk_stock_updater_submitValue(inputElem)
{
	var _parentTR = $($(inputElem).parents("tr").get(0));
	var _sku = _parentTR.children("td:first").text();
	var _name = $(inputElem).attr("name");
	var _value = $(inputElem).val();

	// reset error msgs
	$(inputElem)
		.removeClass("error")
		.after("<div class=\"uc_bulk_stock_updater_progress\"></div>");
	
	$(inputElem).nextAll("div.uc_bulk_stock_updater_ajax_error").remove();
	
	// call
	$.ajax({
		url : Drupal.settings.uc_bulk_stock_updater.ajax_url,
		type: 'POST',
		timeout : 3000,
		data : { sku: _sku, name: _name, value: _value },
		dataType : "json",
	    error : function(_XMLHttpRequest, _textStatus, _errorThrown)
	    {
			uc_bulk_stock_updater_submitValueErr(inputElem, _textStatus);
	    },			
		success : function(_data, _textStatus, _XMLHttpRequest)
		{
	    	if (undefined != _data.error)
	    		uc_bulk_stock_updater_submitValueErr(inputElem, _data.error);
	    	else
	    		$(inputElem).data("original", _value);
	    },
		complete : function(_XMLHttpRequest,_textStatus)
		{
			$(inputElem).nextAll("div.uc_bulk_stock_updater_progress").remove();
		}	    
	});	
}
function uc_bulk_stock_updater_submitValueErr(inputElem, _errorMsg)
{
	$(inputElem)
		.after("<div class=\"uc_bulk_stock_updater_ajax_error\">" + _errorMsg + "</div>")
		.val($(inputElem).data("original"))
		.addClass("error");
}


