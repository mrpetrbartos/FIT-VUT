Feature: Cart

    # 15
    Scenario: Add product to cart
        Given a web browser is at a product page
        And the user selects required options
        When the user clicks on Add to Cart button
        Then the subtotal is updated and notification is displayed

    # 16
    Scenario: Update product quantity
        Given the user navigates to shopping cart
        And the Shopping Cart isn't empty
        When the user updates quantity for a product
        Then the subtotal is updated

    # 17
    Scenario: Remove product from cart
        Given the user navigates to shopping cart
        And the Shopping Cart isn't empty
        When the user removes a product from his shopping cart
        Then the subtotal is updated and product removed

    # 18
    Scenario: Place an order
        Given the Shopping Cart isn't empty
        And the user navigates to Checkout
        And fills in the required details
        When the user clicks on Confirm Order button
        Then the successful order page should be shown
