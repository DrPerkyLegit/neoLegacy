namespace Minecraft.Server.FourKit.Inventory;

using Minecraft.Server.FourKit.Enums;
using System;
using System.Collections.Generic;
using System.Text;


/// <summary>
/// Represents some type of crafting recipe.
/// </summary>
public interface Recipe
{

    /// <summary>
    /// Get the result of this recipe.
    /// </summary>
    /// <returns>The result stack</returns>
    ItemStack getResult();

    /// <summary>
    /// Get the group that this recipe is assigned to
    /// </summary>
    /// <returns>The recipe group type</returns>
    RecipeGroupType getGroupType();

    bool isValid();
}
