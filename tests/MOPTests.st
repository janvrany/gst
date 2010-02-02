Eval [
	(PackageLoader packageAt: #SUnit) fileIn.
]


Object subclass: MockA [
    
    <category: 'Metaobject protocol'>

    foo [    	
		^#foo  
    ]
    
    bar [    	
		^#bar  
    ]
    
    qux [    	
		^#qux  
    ]
    

]

StandardLookup subclass: MockALookup [

	lookup: selector in: initialSearchClass sender: senderClass [		
		(selector == #foo) ifTrue:
			[^self lookup: #bar in: initialSearchClass sender: senderClass].
		(selector == #qux) ifTrue:
			[^#()].			
		^super lookup: selector in: initialSearchClass sender: senderClass
	]

]

TestCase subclass: MOPTests [
	| mockA |
	
	test_01 [
		
		mockA := MockA new.
		self 
			assert: mockA foo = #foo;
			assert: mockA bar = #bar;			  
			assert: mockA qux = #qux.
			
		MockA lookup: MockALookup new.
		
		self 
			assert: mockA foo = #bar;
			assert: mockA bar = #bar.
		
		self should:[mockA qux] raise: Error.						  	
			
		 			
		
	]
]


Eval [
	
	((TestSuite named: 'Metaobject protocol tests')
		addTests: (MOPTests buildSuite tests);
		run) printNl		
]