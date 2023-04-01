/*
 * Binárny vyhľadávací strom — iteratívna varianta
 *
 * S využitím dátových typov zo súboru btree.h, zásobníkov zo súborov stack.h a
 * stack.c a pripravených kostier funkcií implementujte binárny vyhľadávací
 * strom bez použitia rekurzie.
 */

#include "../btree.h"
#include "stack.h"
#include <stdio.h>
#include <stdlib.h>

/*
 * Inicializácia stromu.
 *
 * Užívateľ musí zaistiť, že incializácia sa nebude opakovane volať nad
 * inicializovaným stromom. V opačnom prípade môže dôjsť k úniku pamäte (memory
 * leak). Keďže neinicializovaný ukazovateľ má nedefinovanú hodnotu, nie je
 * možné toto detegovať vo funkcii.
 */
void bst_init(bst_node_t **tree)
{
  *tree = NULL;
}

/*
 * Nájdenie uzlu v strome.
 *
 * V prípade úspechu vráti funkcia hodnotu true a do premennej value zapíše
 * hodnotu daného uzlu. V opačnom prípade funckia vráti hodnotu false a premenná
 * value ostáva nezmenená.
 *
 * Funkciu implementujte iteratívne bez použitia vlastných pomocných funkcií.
 */
bool bst_search(bst_node_t *tree, char key, int *value)
{
  if (tree == NULL || value == NULL)
    return false;

  bst_node_t *currNode = tree;

  while (currNode != NULL)
  {
    if (currNode->key == key)
    {
      *value = currNode->value;
      return true;
    }
    else if (currNode->key > key)
      currNode = currNode->left;
    else
      currNode = currNode->right;
  }

  return false;
}

/*
 * Vloženie uzlu do stromu.
 *
 * Pokiaľ uzol so zadaným kľúčom v strome už existuje, nahraďte jeho hodnotu.
 * Inak vložte nový listový uzol.
 *
 * Výsledný strom musí spĺňať podmienku vyhľadávacieho stromu — ľavý podstrom
 * uzlu obsahuje iba menšie kľúče, pravý väčšie.
 *
 * Funkciu implementujte iteratívne bez použitia vlastných pomocných funkcií.
 */
void bst_insert(bst_node_t **tree, char key, int value)
{
  bst_node_t *currNode = *tree;
  bst_node_t *prevNode = NULL;

  while (currNode)
  {
    prevNode = currNode;
    if (currNode->key > key)
      currNode = currNode->left;
    else if (currNode->key < key)
      currNode = currNode->right;
    else
    {
      currNode->value = value;
      return;
    }
  }

  bst_node_t *newNode = malloc(sizeof(struct bst_node));
  if (newNode == NULL)
    return;

  newNode->key = key;
  newNode->value = value;
  newNode->left = NULL;
  newNode->right = NULL;

  if (prevNode == NULL)
    *tree = newNode;
  else if (prevNode->key > key)
    prevNode->left = newNode;
  else
    prevNode->right = newNode;
}

/*
 * Pomocná funkcia ktorá nahradí uzol najpravejším potomkom.
 *
 * Kľúč a hodnota uzlu target budú nahradené kľúčom a hodnotou najpravejšieho
 * uzlu podstromu tree. Najpravejší potomok bude odstránený. Funkcia korektne
 * uvoľní všetky alokované zdroje odstráneného uzlu.
 *
 * Funkcia predpokladá že hodnota tree nie je NULL.
 *
 * Táto pomocná funkcia bude využitá pri implementácii funkcie bst_delete.
 *
 * Funkciu implementujte iteratívne bez použitia vlastných pomocných funkcií.
 */
void bst_replace_by_rightmost(bst_node_t *target, bst_node_t **tree)
{
  if (target == NULL || *tree == NULL)
    return;

  while ((*tree)->right != NULL)
    tree = &((*tree)->right);

  target->key = (*tree)->key;
  target->value = (*tree)->value;

  bst_node_t *tmp = (*tree)->left;
  free(*tree);
  *tree = tmp;

  return;
}

/*
 * Odstránenie uzlu v strome.
 *
 * Pokiaľ uzol so zadaným kľúčom neexistuje, funkcia nič nerobí.
 * Pokiaľ má odstránený uzol jeden podstrom, zdedí ho otec odstráneného uzla.
 * Pokiaľ má odstránený uzol oba podstromy, je nahradený najpravejším uzlom
 * ľavého podstromu. Najpravejší uzol nemusí byť listom!
 * Funkcia korektne uvoľní všetky alokované zdroje odstráneného uzlu.
 *
 * Funkciu implementujte iteratívne pomocou bst_replace_by_rightmost a bez
 * použitia vlastných pomocných funkcií.
 */
void bst_delete(bst_node_t **tree, char key)
{
  if (*tree == NULL)
    return;

  while (*tree != NULL)
  {
    bst_node_t *currNode = *tree;
    if (currNode->key > key)
      tree = &(currNode->left);
    else if (currNode->key < key)
      tree = &(currNode->right);
    else if (currNode->key == key)
    {
      bst_node_t *tmp = (*tree);

      if (currNode->right == NULL)
      {
        *tree = tmp->left;
        free(tmp);
      }
      else if (currNode->left == NULL)
      {
        *tree = tmp->right;
        free(tmp);
      }
      else
        bst_replace_by_rightmost(currNode, &(currNode->left));
    }
  }
}

/*
 * Zrušenie celého stromu.
 *
 * Po zrušení sa celý strom bude nachádzať v rovnakom stave ako po
 * inicializácii. Funkcia korektne uvoľní všetky alokované zdroje rušených
 * uzlov.
 *
 * Funkciu implementujte iteratívne pomocou zásobníku uzlov a bez použitia
 * vlastných pomocných funkcií.
 */
void bst_dispose(bst_node_t **tree)
{
  if (*tree == NULL)
    return;

  stack_bst_t stack;
  stack_bst_init(&stack);
  bst_node_t *next;

  do
  {
    if (*tree == NULL)
    {
      if (!stack_bst_empty(&stack))
        *tree = stack_bst_pop(&stack);
    }
    else
    {
      if ((*tree)->right != NULL)
        stack_bst_push(&stack, (*tree)->right);

      next = (*tree)->left;
      free(*tree);
      *tree = next;
    }
  } while (*tree != NULL || !stack_bst_empty(&stack));
}

/*
 * Pomocná funkcia pre iteratívny preorder.
 *
 * Prechádza po ľavej vetve k najľavejšiemu uzlu podstromu.
 * Nad spracovanými uzlami zavola bst_print_node a uloží ich do zásobníku uzlov.
 *
 * Funkciu implementujte iteratívne pomocou zásobníku uzlov a bez použitia
 * vlastných pomocných funkcií.
 */
void bst_leftmost_preorder(bst_node_t *tree, stack_bst_t *to_visit)
{
  if (tree == NULL)
    return;

  bst_node_t *currentNode = tree;
  while (currentNode != NULL)
  {
    stack_bst_push(to_visit, currentNode);
    bst_print_node(currentNode);
    currentNode = currentNode->left;
  }
}

/*
 * Preorder prechod stromom.
 *
 * Pre aktuálne spracovávaný uzol nad ním zavolajte funkciu bst_print_node.
 *
 * Funkciu implementujte iteratívne pomocou funkcie bst_leftmost_preorder a
 * zásobníku uzlov bez použitia vlastných pomocných funkcií.
 */
void bst_preorder(bst_node_t *tree)
{
  if (tree == NULL)
    return;

  stack_bst_t stack;
  stack_bst_init(&stack);

  bst_leftmost_preorder(tree, &stack);
  bst_node_t *tmp;

  while (!stack_bst_empty(&stack))
  {
    tmp = stack_bst_top(&stack);
    stack_bst_pop(&stack);
    bst_leftmost_preorder(tmp->right, &stack);
  }
}

/*
 * Pomocná funkcia pre iteratívny inorder.
 *
 * Prechádza po ľavej vetve k najľavejšiemu uzlu podstromu a ukladá uzly do
 * zásobníku uzlov.
 *
 * Funkciu implementujte iteratívne pomocou zásobníku uzlov a bez použitia
 * vlastných pomocných funkcií.
 */
void bst_leftmost_inorder(bst_node_t *tree, stack_bst_t *to_visit)
{
  if (tree == NULL)
    return;

  bst_node_t *currentNode = tree;
  while (currentNode != NULL)
  {
    stack_bst_push(to_visit, currentNode);
    currentNode = currentNode->left;
  }
}

/*
 * Inorder prechod stromom.
 *
 * Pre aktuálne spracovávaný uzol nad ním zavolajte funkciu bst_print_node.
 *
 * Funkciu implementujte iteratívne pomocou funkcie bst_leftmost_inorder a
 * zásobníku uzlov bez použitia vlastných pomocných funkcií.
 */
void bst_inorder(bst_node_t *tree)
{
  if (tree == NULL)
    return;

  stack_bst_t stack;
  stack_bst_init(&stack);

  bst_node_t *top;

  bst_leftmost_inorder(tree, &stack);

  while (!stack_bst_empty(&stack))
  {
    top = stack_bst_pop(&stack);
    bst_print_node(top);
    if (top->right)
      bst_leftmost_inorder(top->right, &stack);
  }
}

/*
 * Pomocná funkcia pre iteratívny postorder.
 *
 * Prechádza po ľavej vetve k najľavejšiemu uzlu podstromu a ukladá uzly do
 * zásobníku uzlov. Do zásobníku bool hodnôt ukladá informáciu že uzol
 * bol navštívený prvý krát.
 *
 * Funkciu implementujte iteratívne pomocou zásobníkov uzlov a bool hodnôt a bez použitia
 * vlastných pomocných funkcií.
 */
void bst_leftmost_postorder(bst_node_t *tree, stack_bst_t *to_visit,
                            stack_bool_t *first_visit)
{
  if (tree == NULL)
    return;

  bst_node_t *currentNode = tree;
  while (currentNode != NULL)
  {
    stack_bst_push(to_visit, currentNode);
    stack_bool_push(first_visit, true);
    currentNode = currentNode->left;
  }
}

/*
 * Postorder prechod stromom.
 *
 * Pre aktuálne spracovávaný uzol nad ním zavolajte funkciu bst_print_node.
 *
 * Funkciu implementujte iteratívne pomocou funkcie bst_leftmost_postorder a
 * zásobníkov uzlov a bool hodnôt bez použitia vlastných pomocných funkcií.
 */
void bst_postorder(bst_node_t *tree)
{
  if (tree == NULL)
    return;

  stack_bst_t nodeStack;
  stack_bst_init(&nodeStack);
  stack_bool_t boolStack;
  stack_bool_init(&boolStack);

  bst_leftmost_postorder(tree, &nodeStack, &boolStack);

  while (!stack_bst_empty(&nodeStack))
  {
    tree = stack_bst_top(&nodeStack);

    if (!stack_bool_pop(&boolStack))
    {
      bst_print_node(tree);
      stack_bst_pop(&nodeStack);
    }
    else
    {
      stack_bool_push(&boolStack, false);
      bst_leftmost_postorder(tree->right, &nodeStack, &boolStack);
    }
  }
}
