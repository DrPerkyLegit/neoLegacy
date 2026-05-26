using Minecraft.Server.FourKit.Enums;
using System;
using System.Collections.Generic;
using System.Security.AccessControl;
using System.Text;

namespace Minecraft.Server.FourKit.Inventory
{
    public class ShapelessRecipe : Recipe
    {
        internal ItemStack _result;
        internal List<ItemStack> _ingredients;
        internal RecipeGroupType _group;


        /// <summary>
        /// Create a shapeless recipe to craft the specified ItemStack. The constructor merely determines the result and type; to set the actual recipe, you'll need to call the appropriate methods.
        /// </summary>
        /// <param name="result">The item you want the recipe to create.</param>
        /// <param name="recipegroup">The group you want this recipe to appear in.</param>
        public ShapelessRecipe(ItemStack result, RecipeGroupType recipegroup)
        {
            _result = result;
            _ingredients = new List<ItemStack>(9);
            _group = recipegroup;
        }


        /// <summary>
        /// Adds the specified ingredient.
        /// </summary>
        /// <param name="ingredient">The ingredient to add.</param>
        /// <returns>The changed recipe, so you can chain calls.</returns>
        public ShapelessRecipe addIngredient(Material ingredient)
        {
            this.addIngredient(1, ingredient, 0);
            return this;
        }

        /// <summary>
        /// Adds the specified ingredient.
        /// </summary>
        /// <param name="ingredient">The ingredient to add.</param>
        /// <param name="rawaux">The aux value.</param>
        /// <returns>The changed recipe, so you can chain calls.</returns>
        public ShapelessRecipe addIngredient(Material ingredient, short rawaux)
        {
            this.addIngredient(1, ingredient, rawaux);
            return this;
        }

        /// <summary>
        /// Adds the specified ingredient.
        /// </summary>
        /// <param name="count">How many to add (can't be more than 9!)</param>
        /// <param name="ingredient">The ingredient to add.</param>
        /// <returns>The changed recipe, so you can chain calls.</returns>
        public ShapelessRecipe addIngredient(int count, Material ingredient)
        {
            this.addIngredient(count, ingredient, 0);
            return this;
        }

        /// <summary>
        /// Adds the specified ingredient.
        /// </summary>
        /// <param name="count">How many to add (can't be more than 9!)</param>
        /// <param name="ingredient">The ingredient to add.</param>
        /// <param name="rawaux">The aux value.</param>
        /// <returns>The changed recipe, so you can chain calls.</returns>
        public ShapelessRecipe addIngredient(int count, Material ingredient, short rawaux)
        {

            for (int i = 0; i < count; i++)
            {
                if (_ingredients.Count >= 9)
                    break;

                _ingredients.Add(new ItemStack(ingredient, 1, rawaux));
            }

            return this;
        }

        /// <summary>
        /// Removes an ingredient from the list. If the ingredient occurs multiple times, only one instance of it is removed. Only removes exact matches, with a data value of 0.
        /// </summary>
        /// <param name="ingredient">The ingredient to remove</param>
        /// <returns>The changed recipe, so you can chain calls.</returns>
        public ShapelessRecipe removeIngredient(Material ingredient)
        {
            this.removeIngredient(1, ingredient, 0);
            return this;
        }

        /// <summary>
        /// Removes an ingredient from the list. If the ingredient occurs multiple times, only one instance of it is removed. If the aux value is -1, only ingredients with a -1 aux value will be removed.
        /// </summary>
        /// <param name="ingredient">The ingredient to remove</param>
        /// <param name="rawaux">The aux value.</param>
        /// <returns>The changed recipe, so you can chain calls.</returns>
        public ShapelessRecipe removeIngredient(Material ingredient, short rawaux)
        {
            this.removeIngredient(1, ingredient, rawaux);
            return this;
        }

        /// <summary>
        /// Removes multiple instances of an ingredient from the list. If there are less instances then specified, all will be removed. Only removes exact matches, with a aux value of 0.
        /// </summary>
        /// <param name="count">The number of copies to remove.</param>
        /// <param name="ingredient">The ingredient to remove.</param>
        /// <returns>The changed recipe, so you can chain calls.</returns>
        public ShapelessRecipe removeIngredient(int count, Material ingredient)
        {
            this.removeIngredient(count, ingredient, 0);
            return this;
        }

        /// <summary>
        /// Removes multiple instances of an ingredient from the list. If there are less instances then specified, all will be removed. If the aux value is -1, only ingredients with a -1 aux value will be removed.
        /// </summary>
        /// <param name="count">The number of copies to remove.</param>
        /// <param name="ingredient">The ingredient to remove.</param>
        /// <param name="rawaux">The aux value.</param>
        /// <returns>The changed recipe, so you can chain calls.</returns>
        public ShapelessRecipe removeIngredient(int count, Material ingredient, short rawaux)
        {
            List<ItemStack> itemsToRemove = new List<ItemStack>();

            foreach (ItemStack item in _ingredients)
            {
                if (item.getType() == ingredient && item.getDurability() == rawaux)
                {
                    itemsToRemove.Add(item);
                }
            }

            if (itemsToRemove.Count > 0)
            {
                for (int i = 0; i < itemsToRemove.Count && i < count; i++)
                {
                    _ingredients.Remove(itemsToRemove[i]);
                }
            }

            return this;
        }

        /// <summary>
        /// Get the list of ingredients used for this recipe.
        /// <returns>A copy of the recipe ingredients array.</returns>
        public List<ItemStack> getIngredientList()
        {
            return new List<ItemStack>(_ingredients);
        }

        /// <inheritdoc/>
        public ItemStack getResult()
        {
            return _result;
        }

        /// <inheritdoc/>
        public RecipeGroupType getGroupType()
        {
            throw new NotImplementedException();
        }

        bool Recipe.isValid()
        {
            if (_result == null)
                return false;

            if (_result.getType() == Material.AIR)
                return false;

            if (_ingredients.Count <= 0)
                return false;

            foreach (ItemStack item in _ingredients)
            {
                if (item == null)
                    return false;

                if (item.getType() == Material.AIR)
                    return false;
            }

            if (_group == RecipeGroupType.eGroupType_Max) 
                return false;

            return true;
        }
    }
}
