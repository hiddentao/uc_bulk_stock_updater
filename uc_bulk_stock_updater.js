// $Id: uc_bulk_stock_updater.js,v 1.2 2010/08/29 18:12:29 hiddentao Exp $


Drupal.behaviors.uc_bulk_stock_updater = function (context) {
	
	// handle item editing
	$("input.uc_bulk_stock_updater_value").each(function(){
		$(this).data("original", $(this).val());
		$(this).change(function(){
			uc_bulk_stock_updater_submitValue(this);
		})
	});

	// filter
	$("#uc_bulk_stock_updater_filter").keyup(function(){
		
		$(this).after("<div class=\"uc_bulk_stock_updater_ajax_progress\"></div>");
		
		var f = $(this).val().toLowerCase();
		if ('' == f)
			$("table.uc-stock-table tr").show();
		else {
			$("table.uc-stock-table tr").each(function(){
				// show 
				if (0 < $(this).find("span[id*=" + f + "]").size()) {
					$(this).show();
				} else {
					$(this).hide();
				}
			});
		}
		
		$(this).next("div.uc_bulk_stock_updater_ajax_progress").remove();		
	});
};


function uc_bulk_stock_updater_submitValue(inputElem)
{
	var _parentTR = $($(inputElem).parents("tr").get(0));
	var _sku = _parentTR.children("td:first").text();
	var _name = $(inputElem).attr("name");
	var _value = $(inputElem).val();

	// reset error msgs
	$(inputElem)
		.removeClass("error")
		.after("<div class=\"uc_bulk_stock_updater_ajax_progress\"></div>");
	
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
			$(inputElem).nextAll("div.uc_bulk_stock_updater_ajax_progress").remove();
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


