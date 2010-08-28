== Bulk stock updater ===

This is an extension module for Ubercart 2.x running on Drupal 6.x which makes it easy to edit product stock levels. Its interface is modelled on the 'Stock report' page displayed by the uc_stock module.

=== Usage ===

Once the module is enabled ensure you assign the 'bulk update stock' permission to the appropriate user roles.

Goto 'Store administration > Stock > Bulk update' and you will see a paged list of your products ordered by SKU. At the bottom is a link to view all the products on one page if you wish.

You will notice that the stock value for each product is displayed as an input field. Once you change the value in an input field it will be automatically submitted to the server via AJAX (you will see a progress indicator while this happens). If any errors occur the field value will be reset to what it was originally and an error message will be shown.

You can refer to the CSS file in the module's folder to find out which styles need to be overridden in order to customize the look and feel.
