Object subclass: MockA [    
    foo [
    	'foo' printNl.    	
		^#foo  
    ]    
    bar [
    	'bar' printNl.    	
		^#bar  
    ]    
    qux [   
    	'qux' printNl. 	
		^#qux  
    ]
]

MockA subclass: MockB [    
    bar [   
    	'bar->baz' printNl. 	
		^#baz 
    ]    
]

Lookup subclass: MockALookup [

	lookup: selector in: initialSearchClass sender: senderClass method: senderMethod [		
		(selector == #foo) ifTrue:
			[^super lookup: #bar in: initialSearchClass sender: senderClass method: senderMethod].
		(selector == #qux) ifTrue:
			[^#()].			
		^super lookup: selector in: initialSearchClass sender: senderClass method: senderMethod
	]

]

TestCase subclass: MOPBasicTest [
	| mock |
	
	setUp [
		MockA lookup: nil.
		MockB lookup: nil.
		Behavior flushCache.
		'--' printNl.
	]
	
	test01a [				
		mock := MockA new.
		self 
			assert: mock foo = #foo;
			assert: mock bar = #bar;			  
			assert: mock qux = #qux.
			
		MockA lookup: MockALookup new.
				
		self 
			assert: mock foo = #bar;
			assert: mock bar = #bar.		
		self should:[mock qux] raise: Error.						  	
	]
	
	test01b [		
		mock := MockB new.
				
		self 
			assert: mock foo = #foo;
			assert: mock bar = #baz;			  
			assert: mock qux = #qux.
			
		MockA lookup: MockALookup new.
				
		self 
			assert: mock foo = #baz;
			assert: mock bar = #baz.		
		self should:[mock qux] raise: Error.						  	
	]
		
	test02 [		
		self assert: (MockA lookup == Lookup builtin).
		MockA lookup: MockALookup new.
		self assert: (MockA lookup class == MockALookup).
		self assert: (MockB lookup class == MockALookup).
	]
]