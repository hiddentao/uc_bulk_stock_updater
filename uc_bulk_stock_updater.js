/*
Bulk stock updater module for Drupal 
Copyright (C) 2010 Ramesh Nair (www.hiddentao.com)

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

var g_attr_orig = "orig";

$(document).ready(function(){
	
	// keep track of which stock levels get edited
	$("input.uc_bulk_stock_updater-value").each(function(){
		$(this).attr(g_attr_orig, $(this).val());
		$(this).change(function(){
			submitStockValue(this);
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
});


function submitStockValue(stockInputElem)
{
	var _sku = $(stockInputElem).attr("name");
	var _stock = $(stockInputElem).val();

	// reset
	$(stockInputElem)
		.removeClass("error")
		.after("<div class=\"uc_bulk_stock_updater_ajax_progress\"></div>");
	
	$(stockInputElem).nextAll("div.uc_bulk_stock_updater_ajax_error").remove();
	
	// call
	$.ajax({
		url : g_uc_bulk_stock_updater_ajax_url,
		type: 'POST',
		timeout : 3000,
		data : { sku: _sku, stock: _stock },
		dataType : "json",
	    error : function(_XMLHttpRequest, _textStatus, _errorThrown)
	    {
			submitStockValueErr(stockInputElem, _textStatus);
	    },			
		success : function(_data, _textStatus, _XMLHttpRequest)
		{
	    	if (undefined != _data.error)
				submitStockValueErr(stockInputElem, _data.error);
	    	else
	    		$(stockInputElem).attr(g_attr_orig, _stock);
	    },
		complete : function(_XMLHttpRequest,_textStatus)
		{
			$(stockInputElem).nextAll("div.uc_bulk_stock_updater_ajax_progress").remove();
		}	    
	});	
}
function submitStockValueErr(stockInputElem, _errorMsg)
{
	$(stockInputElem)
		.after("<div class=\"uc_bulk_stock_updater_ajax_error\">" + _errorMsg + "</div>")
		.val($(stockInputElem).attr(g_attr_orig))
		.addClass("error");
}


