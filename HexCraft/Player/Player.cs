using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityStandardAssets.Utility;
public enum SlotTypes { None, Hand, Chest, Feet, Head, Legs };
public class Player : MonoBehaviour {
    public Dictionary<SlotTypes, Item> equipmentSlots;

    public float moveSpeed = 10;
    int level = 1;
    int curExp = 0;
    int curHealth = 6;
    int maxHealth = 6;
    public int Level { get { return level; } }
    public int CurrentExp { get { return curExp; } }
    public int CurrentHealth { get { return curHealth; } }
    int MaxExp
    {
        get
        {
            return 100 + ((level - 1) * 20);
        }
    }
    public float GetHealthPercent { get { return (float)curHealth / (float)maxHealth; } }
    public float GetEXPPercent { get { return (float)curExp / (float)MaxExp; } }
    public const int numSlots = 25;
    public const int numHandSlots = 6;
    public int currentItemBar = 0;

    public float timeBetweenAttacks = 0.5f;
    public float lastAttack;

    public PlayerInventory inventory;
    //public List<InventoryItem> playerInventory = new List<InventoryItem>();
    //public ItemDatabase playerInventory;
    public BoxCollider resourceCollector;
    IInteractable interactable;
    public bool canMove = true;

    bool initializedPosition = false;
    Vector3 position;
    Quaternion rotation;

    public void Init(Vector3 position, float yRot, int level, int curExp, int curHealth, InventoryItem[] inventory)
    {
        this.position = position;
        this.rotation = Quaternion.Euler(0, yRot, 0);
        this.level = level;
        this.curExp = curExp;
        this.curHealth = curHealth;
        //this.inventory.inventory = inventory;
        this.inventory = new PlayerInventory(numSlots, numHandSlots);
        for (int i = 0; i < inventory.Length; i++)
        {
            this.inventory.AddItemByIndex(i, inventory[i].id, inventory[i].amount);
        }
        if (equipmentSlots[SlotTypes.Hand].Id != this.inventory.inventory[currentItemBar + numSlots].id)
        {
            equipmentSlots[SlotTypes.Hand] = GameManager.itemDatabase.GetByID(this.inventory.inventory[currentItemBar + numSlots].id);
        }
        UpdateHandSlots();
    }

    private void Awake()
    {
        equipmentSlots = new Dictionary<SlotTypes, Item>()
        {
            {SlotTypes.Hand, new Item() },
            { SlotTypes.Chest, new Item() },
            { SlotTypes.Feet, new Item() },
            { SlotTypes.Head, new Item() },
            { SlotTypes.Legs, new Item() }
        };
        GameManager.playerInstance = this;
        inventory = new PlayerInventory(numSlots, numHandSlots);
        lastAttack = timeBetweenAttacks;
    }

    // Start is called before the first frame update
    void Start()
    {
        equipmentSlots[SlotTypes.Hand] = GameManager.itemDatabase.GetByID(inventory.inventory[currentItemBar + numSlots].id);
    }

    // Update is called once per frame
    void Update()
    {
        if(!initializedPosition)
        {
            GetComponent<CharacterController>().transform.position = position + (Vector3.up);
            GetComponent<CharacterController>().transform.rotation = rotation;
            transform.position = position + (Vector3.up);
            transform.rotation = rotation;
            initializedPosition = true;
        }
        if(GameManager.gameState == GameManager.GameState.Exploring && !UIManager.ShowingMenu())
        {
            if (Input.GetMouseButton(0) && lastAttack >= timeBetweenAttacks)
            {
                //do anim for attack
                StartCoroutine(CollectAnim());
            }
            if(Input.GetMouseButton(1) && lastAttack >= timeBetweenAttacks)
            {
                Item item = GameManager.itemDatabase.GetByID(inventory.inventory[currentItemBar + numSlots].id);
                item.RightClick();
                lastAttack = 0;
            }

            //switch itembar index
            if(Input.mouseScrollDelta.y < -0.1f)
            {
                currentItemBar++;
                UpdateHandSlots();
            }
            else if(Input.mouseScrollDelta.y > 0.1f)
            {
                currentItemBar--;
                UpdateHandSlots();
            }
        }
        if (lastAttack <= timeBetweenAttacks)
        {
            lastAttack += Time.deltaTime;
        }
    }

    private void FixedUpdate()
    {
        if(GameManager.gameState == GameManager.GameState.Exploring && !UIManager.ShowingMenu())
        {
            //shoot ray to see what you're looking at
            Ray ray = new Ray(Camera.main.transform.position, Camera.main.transform.forward);
            RaycastHit hit;
            //if(Physics.Raycast(ray, out hit, 2))
            //{
            //    interactable = hit.transform.GetComponent<IInteractable>();
            //}else
            //{
            //    interactable = null;
            //}

            if(Physics.SphereCast(ray, 0.5f, out hit, 2))
            {
                interactable = hit.transform.GetComponent<IInteractable>();
            }
            else
            {
                interactable = null;
            }
        }
    }

    public void UpdateHandSlots()
    {
        if (currentItemBar < 0)
            currentItemBar = 5;
        if (currentItemBar >= 6)
            currentItemBar = 0;
        UIManager.instance.UpdateSelectionIcon(currentItemBar);
        if (equipmentSlots[SlotTypes.Hand].Id != inventory.inventory[currentItemBar + numSlots].id)
        {
            equipmentSlots[SlotTypes.Hand] = GameManager.itemDatabase.GetByID(inventory.inventory[currentItemBar + numSlots].id);
        }
    }

    public void AddExp(int amount)
    {
        curExp += amount;
        UIManager.instance.healthExpBarUI.UpdateUI();
        if (curExp >= MaxExp)
        {
            LevelUp();
        }
    }

    void LevelUp()
    {
        int remainderExp = curExp - MaxExp;
        curExp = 0;
        level++;
        AddExp(remainderExp);
    }

    public void DoDamage(int dmg)
    {
        curHealth -= dmg;
        UIManager.instance.healthExpBarUI.UpdateUI();
        if (curHealth <= 0)
        {
            //die
        }
    }

    //private void OnTriggerEnter(Collider other)
    //{
    //    //Debug.Log("Hit " + other.name);
    //    IInteractable interactable = other.GetComponent<IInteractable>();
        
    //    if(interactable != null)
    //    {
    //        interactable.Use(equipmentSlots[SlotTypes.Hand].Num0, equipmentSlots[SlotTypes.Hand].ToolType);
    //    }
    //}

    IEnumerator CollectAnim()
    {
        //resourceCollector.enabled = true;
        if(interactable != null)
            interactable.Use(equipmentSlots[SlotTypes.Hand].Num0, equipmentSlots[SlotTypes.Hand].ToolType);
        lastAttack = 0;
        yield return new WaitForFixedUpdate();
        //resourceCollector.enabled = false;
        yield break;
    }

    
    public HexagonTypes hexagonSelected = HexagonTypes.Forest;
    public HexagonType hexagon;
    public int hexagonType = 0;
    bool showHexagonTypes = false;
    private void OnGUI()
    {
        GUI.Label(new Rect(Screen.width - 75, 0, 75, 25), "Version " + GameManager.Version);
        GUI.Label(new Rect(Screen.width - 75, 25, 75, 25), FPSCounter.GetFPS());
    }
}