== Bulk stock updater ===

This is an extension module for Ubercart 2.x running on Drupal 6.x which makes it easy to edit product stock levels.

Its interface is modelled on the 'Stock report' page displayed by the uc_stock module. It adds a menu item:

Store administration > Stock > Bulk update

To view the page the user must have the 'administer products' permission (same as for editing Ubercart products).

The page lists all products along with their stock levels and minimum stock thresholds. The stock levels are shown as editable text fields. When the user changes the value within a text field and then takes the focus away from it an AJAX call is made to the server (and handled by uc_bulk_stock_updater) to update the stock level for that particular product with what the user entered.

A progress indicator is shown whilst the AJAX call is taking place and the user is given success/error feedback once complete. If the call fails or the update on the server-side fails then the stock level for the product is set back to its original value. The user can choose to view all products on a single page or opt for a paged view of products (the default view) in case their product catalogue is very large. 