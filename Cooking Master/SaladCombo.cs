using System.Collections;
using System.Collections.Generic;
using UnityEngine;

/// <summary>
/// holds all the combinations for the salad combos
/// </summary>
public class SaladCombo : MonoBehaviour
{
    public static Dictionary<int, List<Vegetables>> combos = new Dictionary<int, List<Vegetables>>()
    {
        { 0, new List<Vegetables>() {Vegetables.Lettuce, Vegetables.Onion } },
        { 1, new List<Vegetables>() {Vegetables.Lettuce, Vegetables.Tomato } },
        { 2, new List<Vegetables>() {Vegetables.Onion, Vegetables.Tomato } },
        { 3, new List<Vegetables>() {Vegetables.Lettuce, Vegetables.Onion, Vegetables.Tomato } }
    };
}
