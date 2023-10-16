Feature: User

    # 08
    Scenario: User doesn't fill all fields when creating an account
        Given the user isn't logged in
        And the user navigates to registration page
        And doesn't fill all required details
        When the user clicks on Continue button
        Then the user is notified that some fields were left empty

    # 09
    Scenario: User can create an account
        Given the user isn't logged in
        And the user navigates to registration page
        And fills in required details
        When the user clicks on Continue button
        Then the user should be logged in to his new account

    # 10
    Scenario: User can't log in to a non-existent account
        Given the user isn't logged in
        And user navigates to Log in page
        And fills in wrong details
        When the user clicks on Login button
        Then the user should be notified that the account doesn't exist

    # 11
    Scenario: User can log in to an existing account
        Given the user isn't logged in
        And the user navigates to Log in page
        And fills in required details
        When the user clicks on Login button
        Then the user should be logged in to his account

    # 12
    Scenario: User can logout of an accont
        Given the user is logged in
        When the user clicks on Logout in My Account section
        Then the user is no longer logged in

    # 13
    Scenario: User can change password
        Given the user is logged in
        And the user navigates to Change password
        And the new password is filled in
        When the user clicks on the Continue button
        Then success notification should be shown

    # 14
    Scenario: User can view order history
        Given the user is logged in
        And the user has placed at least one order
        And the user navigates to Order History
        When the order View button is clicked
        Then details about the order are displayed
